#pragma once
#include ".\stationpredict.h"
#include "DlgTimeMessageBox.h"
#include ".\TrainDepartPredictHolder.h"
#define MAX_PREDICT_STATION_PLAN 64

struct stplannotice
{
	int   flag;
	TRAIN_INDEX train_index;
	IBYTE        rec_index;   
};
class CTgDocument;
class CDepartPredictDlg;
class CTG_TrainScheduleBase;
class CTrainDepartPredictHolder
{
public:
	CTrainDepartPredictHolder(CTgDocument* pDoc, int con_id);
	virtual ~CTrainDepartPredictHolder(void);
	
public:
	void SendStplan(std::vector<stplannotice>& predict_notice, STATION_NO station);
	
	void SaveStplanToHistory();
	bool IsNeedDowithDepartPredict(STATION_NO station);

	BOOL  ProcessPredictByNeibMsg(MsgTrainDepartPredict *pBuf);
	void  ProcessStationReportTime(TRAIN_INDEX train_index, short rec_index, STATION_NO station, ULONG adjust_status, TIME arrive, TIME depart);
	
	void OutputPredict(const TrainPredict& predict);
	bool IsMyNeibStation(STATION_NO station, STATION_NO neib_station);

private:
	void SendRevYugao(MsgTrainDepartPredict *pBuf);
	bool IsMyStation(STATION_NO station);
	
	bool OrganizeStplan(TrainPredict& predict, TRAIN_INDEX train_index, short rec_index, STATION_NO station);

public:
	void LogPredict(short process_id, const char *fmt, ...);
	void NoticeMMIUpdateUI(int msgid, int type, int flag);
	void RegisterMMI(CDepartPredictDlg* pMMI) {	m_pMMI=pMMI;}
	void UnRegisterMMI() {	m_pMMI=NULL;}
	CDepartPredictDlg* GetMMI();
private:
	CDepartPredictDlg* m_pMMI;
	
public:
	int  GetPredictStationList(STATION_NO list[], int num);
	CStationPredict* GetStPredictPlan(STATION_NO this_station);
	CStationPredict* GetStPredictPlan(STATION_NO this_station, STATION_NO neib_station);

private:
	CTG_TrainScheduleBase* m_pWorkSchedule;
	CStationPredict* m_pStPlans[MAX_PREDICT_STATION_PLAN];

private:
	int  m_conid;
	long m_nShiftID;
	CTgDocument* m_pDoc;
};


	

	