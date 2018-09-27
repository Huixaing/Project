#pragma once
#include "stdafx.h"
#include "tg_structdef.h"
#include "tg_schedule_base.h"

typedef unsigned char		SIDE;
typedef unsigned char		ENTRY;
typedef unsigned short		ABS;

class CAdjustBasicParament
{
public:
	void	GetTrainPrio(const TRAIN_NO train, CTG_TrainInfo *traininfo) const;
	short	VerifyStationNumber(STATION_NO sta) const;
	short	VerifyStationSide(STATION_NO sta, SIDE side) const;
	short	IsStopSide(const TG_TrainRecord &rec) const;
	short	IsMainSide(const TG_TrainRecord &rec) const;
	SIDE	GetMainSide(const TG_TrainRecord &rec) const;
	SIDE	GetMainSide(STATION_NO station, ENTRY entry, ENTRY exit) const;
	short	ChoseMainSideWhilePass(CONID con_id);
	short	ChoseSideWhileAllFull(CONID con_id);
	short	GetStopOrderSide(const TG_TrainInfo &coninfo, const TG_TrainRecord &rec, SIDE* side, short *side_count) const;
	short	CalculateTrainRunTime(const TG_TrainRecord &rec, const TG_TrainInfo &info, TRAIN_IN_OUT in_out) const;
	short	CalculateTrainStartExtra(const TG_TrainRecord &rec, const TG_TrainInfo &info, TRAIN_IN_OUT in_out) const;
	short	CalculateTrainStopExtra(const TG_TrainRecord &rec, const TG_TrainInfo &info, TRAIN_IN_OUT in_out) const;
	short	IfConvertToBasicTrain(CONID con_id)  {   return m_ConvertToBasicTrain;}
protected:
private:
	short m_ConvertToBasicTrain;
};