#include "stdafx.h"
#include "tg_work_schdule_holder.h"
#include "tgdocument.h"
#include <baseMsg\msgid.h>
#include <baseMsg\clbth.h>
#include <baseMsg\msg_schd.h>
#include "tg_msgcopy_newctcmsg.h"
#include "tg_function_config.h"
#include "tg_basicpara.h"


IMPORT_C_VISITOR(visitorofNewWintg)
extern  long g_nSourceEntityIDInMessage;

DEFINE_BASEMSG_PROC(MsgNextPlanSendInit, pBufMsg, vpParam, visitorofNewWintg)
{ 	
	CTgDocument::GlobalProcessMessage( g_nSourceEntityIDInMessage,pBufMsg);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}

DEFINE_BASEMSG_PROC(MsgTrainDepartPredict, pBufMsg, vpParam, visitorofNewWintg)
{ 	
	CTgDocument::GlobalProcessMessage( g_nSourceEntityIDInMessage,pBufMsg);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}

DEFINE_BASEMSG_PROC(MsgNextPlanSendComplete, pBufMsg, vpParam, visitorofNewWintg)
{ 	
	CTgDocument::GlobalProcessMessage( g_nSourceEntityIDInMessage,pBufMsg);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}  

DEFINE_BASEMSG_PROC(MsgNextPlanInsert, pBufMsg, vpParam, visitorofNewWintg)
{ 	
	CTgDocument::GlobalProcessMessage( g_nSourceEntityIDInMessage,pBufMsg);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}  


DEFINE_BASEMSG_PROC(MsgNextPlanRecvStatus, pBufMsg, vpParam, visitorofNewWintg)
{ 	
	CTgDocument::GlobalProcessMessage( g_nSourceEntityIDInMessage,pBufMsg);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}  

DEFINE_BASEMSG_PROC(MsgNextPlanSignStatus, pBufMsg, vpParam, visitorofNewWintg)
{ 	
	CTgDocument::GlobalProcessMessage( g_nSourceEntityIDInMessage,pBufMsg);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
} 
DEFINE_BASEMSG_PROC(MsgNextPlanRequest, pBufMsg, vpParam, visitorofNewWintg)
{
	CTgDocument::GlobalProcessMessage( g_nSourceEntityIDInMessage,pBufMsg);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}
DEFINE_BASEMSG_PROC(MsgNoticeNeigbourReceiveSchd, pBufMsg, vpParam, visitorofNewWintg)
{ 	
	CTgDocument::GlobalProcessMessage( g_nSourceEntityIDInMessage,pBufMsg);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
} 
//end  of 发送邻台消息

