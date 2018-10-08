 
/***********************************************************************
            This message id list is for gpc local use
                      define by wjh
                         2004 3.4
***********************************************************************/
#ifndef  __GPC_MSGID_H__
#define __GPC_MSGID_H__

#include "msgid.h"

enum{
 
	HOST_INACTIVE = MSGID_BASE+1, 
    HOST_ACTIVE,
	
	ICON_INIT,
	ICON_DISPLAY,
	ICON_RESET,
	ALARM_DISPLAY,
	TRACE_ALARM_ADD,
	DATA_FILE_LOAD,
	TIME_SYNC,
	UPDATE_TEXT_STATUS,
	INIT_LAYOUT_MESSAGE,
	CLOSE_LAYOUT_MESSAGE,
	CTC_CONTROL_MESSAGE,
	MANUAL_OPERATION,
	DIGITALMAP_2_LAYOUT,

	REPLAY_START,
	REPLAY_TERMINATE,
	REPLAY_DIRECTION,
	REPLAY_MODE,
	REPLAY_NEXT,
	REPLAY_SELECTTIME,
	REPLAY_ACCELERATE,
	REPLAY_SLOW,
	REPLAY_SWITCH_STATUS,
	DOWN_LOAD_FILE_STATUS,

	TG_ENABLE_LAYOUT,
	TG_DISABLE_LAYOUT,

	BUTTON_PRESSED,

	TCC_INITCOMPLETE,
};

#endif //#ifdef  __GPC_MSGID_H__