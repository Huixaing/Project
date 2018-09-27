
#include "Stdafx.h"

#include <BaseMsg/clbth.h>



extern INT WriteBMsgToQueue(USHORT usProcessId, CLBTH::BaseMsg& mMsg, 
	CLBTH::ENTITY_ID dest_entity, USHORT usQueueHandle);

extern INT WriteBMsgToQueue(USHORT usProcessId, 
	CLBTH::BaseMsg* pMsgs[], int num,
	CLBTH::ENTITY_ID dest_entity,
	USHORT usQueueHandle);

