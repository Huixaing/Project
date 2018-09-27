#include "adjustbasicparament.h"

void CAdjustBasicParament::GetTrainPrio(const TRAIN_NO train, CTG_TrainInfo *traininfo) const
{

}

short CAdjustBasicParament::VerifyStationNumber(STATION_NO sta) const
{
	return SUCCESS;
}

short CAdjustBasicParament::VerifyStationSide(STATION_NO sta, SIDE side) const
{
	return SUCCESS;
}

short CAdjustBasicParament::IsStopSide(const TG_TrainRecord &rec) const
{
	return SUCCESS;
}

short CAdjustBasicParament::IsMainSide(const TG_TrainRecord &rec) const
{
	return SUCCESS;
}

SIDE CAdjustBasicParament::GetMainSide(const TG_TrainRecord &rec) const
{
	return GetMainSide(rec.station, rec.entry, rec.exit);
}

SIDE CAdjustBasicParament::GetMainSide(STATION_NO station, ENTRY entry, ENTRY exit) const
{
	return NULL_SIDE;
}

short CAdjustBasicParament::GetStopOrderSide(const TG_TrainInfo &coninfo, const TG_TrainRecord &rec, SIDE* side, short *side_count) const
{
	return SUCCESS;
}

short CAdjustBasicParament::ChoseMainSideWhilePass(CONID con_id)
{
	return SUCCESS;
}

short CAdjustBasicParament::ChoseSideWhileAllFull(CONID con_id)
{
	return SUCCESS;
}

short	CAdjustBasicParament::CalculateTrainRunTime(const TG_TrainRecord &rec, const TG_TrainInfo &info, TRAIN_IN_OUT in_out) const
{
	return SUCCESS;
}
short	CAdjustBasicParament::CalculateTrainStartExtra(const TG_TrainRecord &rec, const TG_TrainInfo &info, TRAIN_IN_OUT in_out) const
{
	return SUCCESS;
}

short	CAdjustBasicParament::CalculateTrainStopExtra(const TG_TrainRecord &rec, const TG_TrainInfo &info, TRAIN_IN_OUT in_out) const
{
	return SUCCESS;
}