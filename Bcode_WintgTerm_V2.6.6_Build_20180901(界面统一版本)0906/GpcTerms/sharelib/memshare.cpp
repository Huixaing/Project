/****************************************************************************
 The shared memory in the system is passed by name to where
 all of the common variables can be accessed and refrenced by name. this dll
 memory section will only be loaded once in the memory and initialized bye
 the os only once.

 The following modules define all of the static memory locations in the
 CAD system. Adding more information into this file requires the
 recompilation of all modules.
****************************************************************************/

#include "stdafx.h"
#include "gpcdef.h"
#include "memshare.h"

CGpcdef   gpcdef;
struct  ThreadStruct PMTG;
struct  ThreadStruct TGMSG;
struct  ThreadStruct PLOT;
void dummy(void)
{
}
