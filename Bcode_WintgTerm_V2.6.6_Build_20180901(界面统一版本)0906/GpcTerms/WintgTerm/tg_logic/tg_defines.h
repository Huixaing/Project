#ifndef __INCLUDE_TG_DEFINES__
#define __INCLUDE_TG_DEFINES__
#include "Basemsg\ctc_defines.h"
#include "defines.h"
using namespace CLBTH;

#define MAX_TRAIN_RUN_TYPE  16
#define MAX_ENTRY_COUNT     32
#define MAX_SIDE_COUNT      256  // 最大的股道数目--涉及支持1-254个股道计算不能小于256

#define MAX_PRIORITY_VALUE  16

const TIME        NULL_TIME = (TIME)-1;
const CONID       NULL_CONID = (CONID)-1;
const ENTITY_ID   NULL_ENTITY=INVALID_ENTITY_ID;
const STATION_NO  NULL_STATION=INVALID_STATION_NO;
const DMIS_ID     NULL_DMISID=INVALID_DMIS_ID;
const DEVICE_TYPE NULL_DEVICETYPE=(DEVICE_TYPE)-1;
const TRAIN_INDEX NULL_TRAININDEX=NULL_TRAIN_INDEX;
const USHORT      NULL_PATHWAY=(USHORT)-1;
const BYTE        NULL_SCHDTYPE=(BYTE)-1;
const ENTRY_ID    NULL_ENTRY=(ENTRY_ID)-1;
const SIDE_ID     NULL_SIDE=(SIDE_ID)-1;
const SHIFT_ID    NULL_SHIFTID=(SHIFT_ID)-1;
const SHUNT_INDEX NULL_SHUNTINDEX=(SHUNT_INDEX)0;
const USHORT      NULL_STAELEMENT=(USHORT)-1;
const short       NULL_ELECARMID = -1;
const BYTE        NULL_TRAINTYPE=0;
#define   TRAIN_ARRIVE              1
#define   TRAIN_DEPART              2

#define  IS_VALID_ENTRY(x)  (x>0 && x<100)
#define  IS_VALID_SIDE(x)  (x>0 && x<255)

#define  TG_FAILED    0
#define  TG_SUCCESS    1
#define  TG_NO_MORE_DATA  2
#endif