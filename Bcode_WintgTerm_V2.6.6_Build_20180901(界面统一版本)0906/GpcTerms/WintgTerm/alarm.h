#ifndef _CONFLICT_
#define _CONFLICT_

#include "dbfunc.h"

#define  STA_ENTRY           0
#define  STA_SIDE            1
#define  STA_EXIT            2
#define  STA_ABS             3
#define  STA_THROAT          4
#define  STA_SPECIAL_ENTRY   5


Pstruct _Conflict_Alarm_Stru {
	unsigned int sequence;
	TRAIN_CONFLICT_ELE first;
	TRAIN_CONFLICT_ELE second;
};

#define MAX_CONFLICT_ALARM  1000

Pstruct _System_Alarm_Stru {
  unsigned char sequence;
  char message[256];
  };

#define MAX_SYSTEM_ALARM  64

#define SYSTEM_SHUTDOWN              0x0f

#define SYSTEM_ALARM_LOG_APPEND      (WM_USER + 300)
#define SYSTEM_ALARM_LOG_DELETE      (WM_USER + 301)
#define SYSTEM_ALARM_LOG_INSERT      (WM_USER + 302)

/*----msg from tgmsg to conflict window----*/
#define CONFLICT_ALARM_LOG_APPEND      0
#define CONFLICT_ALARM_LOG_DELETE      1
#define CONFLICT_ALARM_LOG_INSERT      2
#define CONFLICT_ALARM_DELETE_ALL      3
#define CONFLICT_ALARM_LOG_END         4
#define CONFLICT_ALARM_UNSELECT        5
#define CONFLICT_ALARM_SELECT          6


/*----msg from conflict window to tgmsg----*/
#define CONFLICT_TRAINS             0x8100

#endif