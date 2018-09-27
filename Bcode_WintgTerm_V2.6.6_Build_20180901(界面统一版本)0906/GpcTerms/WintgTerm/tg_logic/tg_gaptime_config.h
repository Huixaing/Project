#ifndef GAPTIME_FILE_H
#define GAPTIME_FILE_H
#include "tg_structdef.h"
#include <vector>
using namespace std;
struct AdjustGapTime{
	   STATION_NO  station;
	   GAP_TYPE    gap_type;
	   ARRIVE_DEPART_STATUS                 act1;
	   ARRIVE_DEPART_STATUS          act2;
	   RUN_TYPE              run_type1;
	   RUN_TYPE                  run_type2;
	   int                          gap_time;
	   AdjustGapTime()
	   {
		   station=0;
		   gap_type=NO_GAP;
		   act1=STATION_NULLSTATUS;
		   act2=STATION_NULLSTATUS;
		   run_type1=NULL_RUNTYPE;
		   run_type2=NULL_RUNTYPE;
		   gap_time=0;
      }
   };

class  CGetGapTimeInfor
{
   vector<AdjustGapTime>  m_aAdjustGapTimeConfig;
public:
	//CGetGapTimeInfor();
	//~CGetGapTimeInfor();
	BOOL  LoadFromXML(const char *pathname);
	int   GetTrainGapTime(STATION_NO station, GAP_TYPE gap_type, ARRIVE_DEPART_STATUS act1, ARRIVE_DEPART_STATUS act2, RUN_TYPE run_type1, RUN_TYPE run_type2) const;
private:
};

#endif