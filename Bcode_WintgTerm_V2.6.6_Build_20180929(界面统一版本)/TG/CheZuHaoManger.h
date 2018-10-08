#pragma once

#include "Tg_TRAIN.h"
#include <vector>  

using namespace std ; 

const int MAX_CZH_NUM = 2048;
struct TRAIN_CZH_INFO
{
	ULONG train_index;
	int   begin_station;
	int   end_station;
	ULONG stTime, edTime;
	TRAIN_NO trainid;
	
	TRAIN_CZH_INFO()
	{
		train_index=0;
		begin_station=0;
		end_station=0;
		stTime=0;
		edTime=0;
		memset(trainid, 0, 10);
	}
};

class TRAIN_CHEZUHAO
{
public:
	TRAIN_CHEZUHAO();
	~TRAIN_CHEZUHAO();

public:
	void InitCheZuHao();
	bool TrainIsJiaoLu(TRAIN_INDEX index);
	void SetRect(int l, int t, int r, int b);
public:
	CString czh;
	CRect rect;

	std::map<ULONG, TRAIN_CZH_INFO> trains;
};


class CTGDoc;
class CTG_Mange;
class CCheZuHaoManger
{
public:
	CCheZuHaoManger();
	virtual ~CCheZuHaoManger(void);

public:
	void ClearAllTrainCheHao();

	bool DeleteCheZuHao(CTG_TRAIN& train);
	bool UpdateCheZuHao(CTG_TRAIN& train);
	bool AppendCheZuHao(CTG_TRAIN& train);

	bool AppendCheZuHaoByRelation(CTG_TRAIN& train, int key, CString& czh);
	void UpdateCheZuHaoByRelation(CTG_TRAIN& train);
	bool GenCZHInfoFromTrain(CTG_TRAIN& train_org, TRAIN_CZH_INFO& info);
	bool GetReturnTrainID(CTG_TRAIN* single, CString& strReturnID);
	
	bool CheckTrainCZHValid(CTG_TRAIN* train, CString& str);
	TRAIN_CHEZUHAO* GetNewCzh();
	bool SetReturnTrainId(TRAIN_CHEZUHAO* pTrainsCZH, CStringArray& result);

	CTG_TRAIN* GetPreTrain(CTG_TRAIN* train);
	CTG_TRAIN* GetNextTrain(CTG_TRAIN* train);
private:
	void NoticeCzhDlg();
	bool DeleteFromCZHQueue(TRAIN_CHEZUHAO* pTrainsCZH, long train_index);
	
	bool GenCZHInfoFromTrain(CTG_TRAIN& train, TRAIN_CZH_INFO& info, ULONG& key, CString& czh);
	
	
	bool SetTrainReturnID(std::map<ULONG, TRAIN_CZH_INFO>::iterator it, std::map<ULONG, TRAIN_CZH_INFO>::iterator next_it, CString& info);
public:
	TRAIN_CHEZUHAO* GetCzhByCheZuHao(const CString& czh);
	TRAIN_CHEZUHAO* GetCzhByTrainIndex(TRAIN_INDEX trainIndex);
	
	void DeleteCzhByCheZuHao(const CString& czh);
	int  GetTrainCZHText(CStringArray& array);
	int  GetSameCZTrainIndexList(TRAIN_CHEZUHAO* pSelTrainCZH, long nCurrentSelTrainIndex, std::vector<long>& list, long& nLeftTime, int& nLeftStation, CString& trainid);
	long GetCzhTrainStartTm(TRAIN_CHEZUHAO* pSelTrainCZH, long index, CString& trainid,int& nRightStation);
public:
	UINT GetTrainCheZuHaoNum();
	
	void SetCZhRight(CTGDoc* pDoc);
	void OutputCZRelation(); // 北京局打印列车时需要同车组列车关系, 在此输出车组关系文件
	
public:
	UINT m_TrainCheZuHaoNum;
	TRAIN_CHEZUHAO m_TrainCheZuHao[MAX_CZH_NUM];

private:
	CTGDoc* m_pDoc;
	vector<CTG_TRAIN> m_newtrain_list;
};
