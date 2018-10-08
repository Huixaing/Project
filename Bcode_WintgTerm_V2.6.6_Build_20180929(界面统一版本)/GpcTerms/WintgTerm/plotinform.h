#ifndef _PRINT_INFORM_HEADER
#define _PRINT_INFORM_HEADER
#include "defines.h"

Pstruct _tagPrintInform {

	ushort usSchdlType;
	ushort usSectionNo;
	long usStartDate;
	long usStartTime;
	long usEndDate;
	long usEndTime;
	unsigned long usShiftid;
	int sBasicFlag; // 1:18-06 2:06-18 0:24-24
};

typedef  Pstruct _tagPrintInform PRINTINFORM;
#endif