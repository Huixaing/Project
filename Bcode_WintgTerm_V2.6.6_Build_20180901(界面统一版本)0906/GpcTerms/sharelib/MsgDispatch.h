
#ifndef __MSGDISPATCH_H__
#define __MSGDISPATCH_H__

#include "isocket.h"

#include <list>
using namespace std;


typedef IBOOL (*MSGRECEIVERPROC)(
	IWORD nTaskId,
	const char* szSenderName,
	IDWORD  dwUserPriority,
	const L3_MSG_HEADER& mL3Head,
	void *pData, int len);

struct  MSGRECEIVER_ENTRY {
	IDWORD dwFlag;
	MSGRECEIVERPROC receive;
	MSGRECEIVER_ENTRY(IDWORD dw, MSGRECEIVERPROC rec)
		: dwFlag(dw), receive(rec) {}
};
typedef list<MSGRECEIVER_ENTRY> MSGRECEIVER_MAP;
inline int MSGRECEIVER_MAP_push_back(MSGRECEIVER_MAP& receivers, MSGRECEIVER_ENTRY receiver)
{
	receivers.push_back(receiver);
	return 0;
}

#define DECLARE_MSGRECEIVERLIST(comm) \
	__declspec(dllexport) MSGRECEIVER_MAP& getMsgReceivers_##comm() \
	{ \
		static MSGRECEIVER_MAP receivers; \
		return receivers; \
	}

#define IMPORT_MSGRECEIVERLIST(comm) \
	__declspec(dllimport) MSGRECEIVER_MAP& getMsgReceivers_##comm();

#define GET_MSGRECEIVERLIST(comm) getMsgReceivers_##comm()

inline int DispatchMsg(
		MSGRECEIVER_MAP& receivers,
		IWORD nTaskId, 
		const char* szSenderName, 
		IDWORD  dwUserPriority, 
		const L3_MSG_HEADER& mL3Head, 
		void *pData, int len)
{
	IBOOL nRet = 0;

	MSGRECEIVER_MAP::iterator iter = receivers.begin();
	for(; iter != receivers.end(); iter ++)
	{
		if( !(dwUserPriority & (*iter).dwFlag) )
			continue;

		nRet = (*iter).receive(nTaskId, szSenderName, dwUserPriority,
			mL3Head, pData, len);
		if(!nRet)
			break;
	}
	return nRet;
}

#define DECLARE_MSGRECEIVER(receivers, receiveproc, dwFlag) \
	IMPORT_MSGRECEIVERLIST(receivers) \
	int __nouse##receivers##receiveproc = \
		MSGRECEIVER_MAP_push_back( \
			GET_MSGRECEIVERLIST(receivers), \
			MSGRECEIVER_ENTRY((DWORD)dwFlag, \
				static_cast<MSGRECEIVERPROC>(receiveproc) \
			) \
		);

#endif //#ifndef __MSGDISPATCH_H__