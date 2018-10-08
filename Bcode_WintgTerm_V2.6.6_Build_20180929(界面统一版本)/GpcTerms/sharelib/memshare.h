#ifndef MEMSHARE_INC
#define MEMSHARE_INC

#include <defines.h>
#include "ComWindow.h"
#include "gpcdef.h"

extern CGpcdef gpcdef;
extern CComWindow g_cMainComWindow;


/// 仅仅是兼容性考虑
struct  ThreadStruct{
	int  ThreadId;
};

extern struct  ThreadStruct PMTG;
extern struct  ThreadStruct TGMSG;
extern struct  ThreadStruct PLOT;	

#endif
